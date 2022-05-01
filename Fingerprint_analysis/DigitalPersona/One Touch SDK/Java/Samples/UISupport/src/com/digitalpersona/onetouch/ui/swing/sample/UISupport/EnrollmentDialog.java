package com.digitalpersona.onetouch.ui.swing.sample.UISupport;

import com.digitalpersona.onetouch.*;
import com.digitalpersona.onetouch.ui.swing.*;

import javax.swing.*;

import java.awt.*;
import java.awt.event.*;
import java.util.*;

/**
 * Enrollment control test 
 */
public class EnrollmentDialog
	extends JDialog
{
    private EnumMap<DPFPFingerIndex, DPFPTemplate> templates;

    public EnrollmentDialog(Frame owner, int maxCount, final String reasonToFail, EnumMap<DPFPFingerIndex, DPFPTemplate> templates) {
        super (owner, true);
        this.templates = templates;

        setTitle("Fingerprint Enrollment");

        DPFPEnrollmentControl enrollmentControl = new DPFPEnrollmentControl();

        EnumSet<DPFPFingerIndex> fingers = EnumSet.noneOf(DPFPFingerIndex.class);
        fingers.addAll(templates.keySet());
        enrollmentControl.setEnrolledFingers(fingers);
        enrollmentControl.setMaxEnrollFingerCount(maxCount);
        
        enrollmentControl.addEnrollmentListener(new DPFPEnrollmentListener()
        {
            public void fingerDeleted(DPFPEnrollmentEvent e) throws DPFPEnrollmentVetoException {
            	if (reasonToFail != null)
            		throw new DPFPEnrollmentVetoException(reasonToFail);
            	else
            	if (JOptionPane.showConfirmDialog(EnrollmentDialog.this,
            		"Are you sure you want to delete the " + Utilities.fingerprintName(e.getFingerIndex()) + "?", "Fingerprint Enrollment",
					JOptionPane.YES_NO_OPTION) == JOptionPane.NO_OPTION
				)
            		throw new DPFPEnrollmentVetoException();
            	else
            		EnrollmentDialog.this.templates.remove(e.getFingerIndex());
			}

            public void fingerEnrolled(DPFPEnrollmentEvent e) throws DPFPEnrollmentVetoException {
            	if (reasonToFail != null) {
//            		e.setStopCapture(false);
            		throw new DPFPEnrollmentVetoException(reasonToFail);
            	} else
            		EnrollmentDialog.this.templates.put(e.getFingerIndex(), e.getTemplate());
            }
        });

		getContentPane().setLayout(new BorderLayout());

		JButton closeButton = new JButton("Close");
        closeButton.addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent e) {
                setVisible(false);                //End Dialog
            }
        });

		JPanel bottom = new JPanel();
		bottom.add(closeButton);
		add(enrollmentControl, BorderLayout.CENTER);
		add(bottom, BorderLayout.PAGE_END);

		pack();
        setLocationRelativeTo(null);         
   }
}